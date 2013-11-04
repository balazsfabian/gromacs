/*
 * This file is part of the GROMACS molecular simulation package.
 *
 * Copyright (c) 2012, by the GROMACS development team, led by
 * David van der Spoel, Berk Hess, Erik Lindahl, and including many
 * others, as listed in the AUTHORS file in the top-level source
 * directory and at http://www.gromacs.org.
 *
 * GROMACS is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * GROMACS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GROMACS; if not, see
 * http://www.gnu.org/licenses, or write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.
 *
 * If you want to redistribute modifications to GROMACS, please
 * consider that scientific software is very special. Version
 * control is crucial - bugs must be traceable. We will be happy to
 * consider code for inclusion in the official distribution, but
 * derived work must not be called official GROMACS. Details are found
 * in the README & COPYING files - if they are missing, get the
 * official version at http://www.gromacs.org.
 *
 * To help us fund GROMACS development, we humbly ask that you cite
 * the research papers on the package. Check out http://www.gromacs.org.
 */
/*
 * Note: this file was generated by the GROMACS sparc64_hpc_ace_double kernel generator.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>

#include "../nb_kernel.h"
#include "types/simple.h"
#include "vec.h"
#include "nrnb.h"

#include "kernelutil_sparc64_hpc_ace_double.h"

/*
 * Gromacs nonbonded kernel:   nb_kernel_ElecRF_VdwLJ_GeomW3P1_VF_sparc64_hpc_ace_double
 * Electrostatics interaction: ReactionField
 * VdW interaction:            LennardJones
 * Geometry:                   Water3-Particle
 * Calculate force/pot:        PotentialAndForce
 */
void
nb_kernel_ElecRF_VdwLJ_GeomW3P1_VF_sparc64_hpc_ace_double
                    (t_nblist * gmx_restrict                nlist,
                     rvec * gmx_restrict                    xx,
                     rvec * gmx_restrict                    ff,
                     t_forcerec * gmx_restrict              fr,
                     t_mdatoms * gmx_restrict               mdatoms,
                     nb_kernel_data_t * gmx_restrict        kernel_data,
                     t_nrnb * gmx_restrict                  nrnb)
{
    /* Suffixes 0,1,2,3 refer to particle indices for waters in the inner or outer loop, or
     * just 0 for non-waters.
     * Suffixes A,B refer to j loop unrolling done with double precision SIMD, e.g. for the two different
     * jnr indices corresponding to data put in the four positions in the SIMD register.
     */
    int              i_shift_offset,i_coord_offset,outeriter,inneriter;
    int              j_index_start,j_index_end,jidx,nri,inr,ggid,iidx;
    int              jnrA,jnrB;
    int              j_coord_offsetA,j_coord_offsetB;
    int              *iinr,*jindex,*jjnr,*shiftidx,*gid;
    real             rcutoff_scalar;
    real             *shiftvec,*fshift,*x,*f;
    _fjsp_v2r8       tx,ty,tz,fscal,rcutoff,rcutoff2,jidxall;
    int              vdwioffset0;
    _fjsp_v2r8       ix0,iy0,iz0,fix0,fiy0,fiz0,iq0,isai0;
    int              vdwioffset1;
    _fjsp_v2r8       ix1,iy1,iz1,fix1,fiy1,fiz1,iq1,isai1;
    int              vdwioffset2;
    _fjsp_v2r8       ix2,iy2,iz2,fix2,fiy2,fiz2,iq2,isai2;
    int              vdwjidx0A,vdwjidx0B;
    _fjsp_v2r8       jx0,jy0,jz0,fjx0,fjy0,fjz0,jq0,isaj0;
    _fjsp_v2r8       dx00,dy00,dz00,rsq00,rinv00,rinvsq00,r00,qq00,c6_00,c12_00;
    _fjsp_v2r8       dx10,dy10,dz10,rsq10,rinv10,rinvsq10,r10,qq10,c6_10,c12_10;
    _fjsp_v2r8       dx20,dy20,dz20,rsq20,rinv20,rinvsq20,r20,qq20,c6_20,c12_20;
    _fjsp_v2r8       velec,felec,velecsum,facel,crf,krf,krf2;
    real             *charge;
    int              nvdwtype;
    _fjsp_v2r8       rinvsix,rvdw,vvdw,vvdw6,vvdw12,fvdw,fvdw6,fvdw12,vvdwsum,sh_vdw_invrcut6;
    int              *vdwtype;
    real             *vdwparam;
    _fjsp_v2r8       one_sixth   = gmx_fjsp_set1_v2r8(1.0/6.0);
    _fjsp_v2r8       one_twelfth = gmx_fjsp_set1_v2r8(1.0/12.0);
    _fjsp_v2r8       itab_tmp;
    _fjsp_v2r8       dummy_mask,cutoff_mask;
    _fjsp_v2r8       one     = gmx_fjsp_set1_v2r8(1.0);
    _fjsp_v2r8       two     = gmx_fjsp_set1_v2r8(2.0);
    union { _fjsp_v2r8 simd; long long int i[2]; } vfconv,gbconv,ewconv;

    x                = xx[0];
    f                = ff[0];

    nri              = nlist->nri;
    iinr             = nlist->iinr;
    jindex           = nlist->jindex;
    jjnr             = nlist->jjnr;
    shiftidx         = nlist->shift;
    gid              = nlist->gid;
    shiftvec         = fr->shift_vec[0];
    fshift           = fr->fshift[0];
    facel            = gmx_fjsp_set1_v2r8(fr->epsfac);
    charge           = mdatoms->chargeA;
    krf              = gmx_fjsp_set1_v2r8(fr->ic->k_rf);
    krf2             = gmx_fjsp_set1_v2r8(fr->ic->k_rf*2.0);
    crf              = gmx_fjsp_set1_v2r8(fr->ic->c_rf);
    nvdwtype         = fr->ntype;
    vdwparam         = fr->nbfp;
    vdwtype          = mdatoms->typeA;

    /* Setup water-specific parameters */
    inr              = nlist->iinr[0];
    iq0              = _fjsp_mul_v2r8(facel,gmx_fjsp_set1_v2r8(charge[inr+0]));
    iq1              = _fjsp_mul_v2r8(facel,gmx_fjsp_set1_v2r8(charge[inr+1]));
    iq2              = _fjsp_mul_v2r8(facel,gmx_fjsp_set1_v2r8(charge[inr+2]));
    vdwioffset0      = 2*nvdwtype*vdwtype[inr+0];

    /* Avoid stupid compiler warnings */
    jnrA = jnrB = 0;
    j_coord_offsetA = 0;
    j_coord_offsetB = 0;

    outeriter        = 0;
    inneriter        = 0;

    /* Start outer loop over neighborlists */
    for(iidx=0; iidx<nri; iidx++)
    {
        /* Load shift vector for this list */
        i_shift_offset   = DIM*shiftidx[iidx];

        /* Load limits for loop over neighbors */
        j_index_start    = jindex[iidx];
        j_index_end      = jindex[iidx+1];

        /* Get outer coordinate index */
        inr              = iinr[iidx];
        i_coord_offset   = DIM*inr;

        /* Load i particle coords and add shift vector */
        gmx_fjsp_load_shift_and_3rvec_broadcast_v2r8(shiftvec+i_shift_offset,x+i_coord_offset,
                                                 &ix0,&iy0,&iz0,&ix1,&iy1,&iz1,&ix2,&iy2,&iz2);

        fix0             = _fjsp_setzero_v2r8();
        fiy0             = _fjsp_setzero_v2r8();
        fiz0             = _fjsp_setzero_v2r8();
        fix1             = _fjsp_setzero_v2r8();
        fiy1             = _fjsp_setzero_v2r8();
        fiz1             = _fjsp_setzero_v2r8();
        fix2             = _fjsp_setzero_v2r8();
        fiy2             = _fjsp_setzero_v2r8();
        fiz2             = _fjsp_setzero_v2r8();

        /* Reset potential sums */
        velecsum         = _fjsp_setzero_v2r8();
        vvdwsum          = _fjsp_setzero_v2r8();

        /* Start inner kernel loop */
        for(jidx=j_index_start; jidx<j_index_end-1; jidx+=2)
        {

            /* Get j neighbor index, and coordinate index */
            jnrA             = jjnr[jidx];
            jnrB             = jjnr[jidx+1];
            j_coord_offsetA  = DIM*jnrA;
            j_coord_offsetB  = DIM*jnrB;

            /* load j atom coordinates */
            gmx_fjsp_load_1rvec_2ptr_swizzle_v2r8(x+j_coord_offsetA,x+j_coord_offsetB,
                                              &jx0,&jy0,&jz0);

            /* Calculate displacement vector */
            dx00             = _fjsp_sub_v2r8(ix0,jx0);
            dy00             = _fjsp_sub_v2r8(iy0,jy0);
            dz00             = _fjsp_sub_v2r8(iz0,jz0);
            dx10             = _fjsp_sub_v2r8(ix1,jx0);
            dy10             = _fjsp_sub_v2r8(iy1,jy0);
            dz10             = _fjsp_sub_v2r8(iz1,jz0);
            dx20             = _fjsp_sub_v2r8(ix2,jx0);
            dy20             = _fjsp_sub_v2r8(iy2,jy0);
            dz20             = _fjsp_sub_v2r8(iz2,jz0);

            /* Calculate squared distance and things based on it */
            rsq00            = gmx_fjsp_calc_rsq_v2r8(dx00,dy00,dz00);
            rsq10            = gmx_fjsp_calc_rsq_v2r8(dx10,dy10,dz10);
            rsq20            = gmx_fjsp_calc_rsq_v2r8(dx20,dy20,dz20);

            rinv00           = gmx_fjsp_invsqrt_v2r8(rsq00);
            rinv10           = gmx_fjsp_invsqrt_v2r8(rsq10);
            rinv20           = gmx_fjsp_invsqrt_v2r8(rsq20);

            rinvsq00         = _fjsp_mul_v2r8(rinv00,rinv00);
            rinvsq10         = _fjsp_mul_v2r8(rinv10,rinv10);
            rinvsq20         = _fjsp_mul_v2r8(rinv20,rinv20);

            /* Load parameters for j particles */
            jq0              = gmx_fjsp_load_2real_swizzle_v2r8(charge+jnrA+0,charge+jnrB+0);
            vdwjidx0A        = 2*vdwtype[jnrA+0];
            vdwjidx0B        = 2*vdwtype[jnrB+0];

            fjx0             = _fjsp_setzero_v2r8();
            fjy0             = _fjsp_setzero_v2r8();
            fjz0             = _fjsp_setzero_v2r8();

            /**************************
             * CALCULATE INTERACTIONS *
             **************************/

            /* Compute parameters for interactions between i and j atoms */
            qq00             = _fjsp_mul_v2r8(iq0,jq0);
            gmx_fjsp_load_2pair_swizzle_v2r8(vdwparam+vdwioffset0+vdwjidx0A,
                                         vdwparam+vdwioffset0+vdwjidx0B,&c6_00,&c12_00);

            /* REACTION-FIELD ELECTROSTATICS */
            velec            = _fjsp_mul_v2r8(qq00,_fjsp_sub_v2r8(_fjsp_madd_v2r8(krf,rsq00,rinv00),crf));
            felec            = _fjsp_mul_v2r8(qq00,_fjsp_msub_v2r8(rinv00,rinvsq00,krf2));

            /* LENNARD-JONES DISPERSION/REPULSION */

            rinvsix          = _fjsp_mul_v2r8(_fjsp_mul_v2r8(rinvsq00,rinvsq00),rinvsq00);
            vvdw6            = _fjsp_mul_v2r8(c6_00,rinvsix);
            vvdw12           = _fjsp_mul_v2r8(c12_00,_fjsp_mul_v2r8(rinvsix,rinvsix));
            vvdw             = _fjsp_msub_v2r8( vvdw12,one_twelfth, _fjsp_mul_v2r8(vvdw6,one_sixth) );
            fvdw             = _fjsp_mul_v2r8(_fjsp_sub_v2r8(vvdw12,vvdw6),rinvsq00);

            /* Update potential sum for this i atom from the interaction with this j atom. */
            velecsum         = _fjsp_add_v2r8(velecsum,velec);
            vvdwsum          = _fjsp_add_v2r8(vvdwsum,vvdw);

            fscal            = _fjsp_add_v2r8(felec,fvdw);

            /* Update vectorial force */
            fix0             = _fjsp_madd_v2r8(dx00,fscal,fix0);
            fiy0             = _fjsp_madd_v2r8(dy00,fscal,fiy0);
            fiz0             = _fjsp_madd_v2r8(dz00,fscal,fiz0);
            
            fjx0             = _fjsp_madd_v2r8(dx00,fscal,fjx0);
            fjy0             = _fjsp_madd_v2r8(dy00,fscal,fjy0);
            fjz0             = _fjsp_madd_v2r8(dz00,fscal,fjz0);

            /**************************
             * CALCULATE INTERACTIONS *
             **************************/

            /* Compute parameters for interactions between i and j atoms */
            qq10             = _fjsp_mul_v2r8(iq1,jq0);

            /* REACTION-FIELD ELECTROSTATICS */
            velec            = _fjsp_mul_v2r8(qq10,_fjsp_sub_v2r8(_fjsp_madd_v2r8(krf,rsq10,rinv10),crf));
            felec            = _fjsp_mul_v2r8(qq10,_fjsp_msub_v2r8(rinv10,rinvsq10,krf2));

            /* Update potential sum for this i atom from the interaction with this j atom. */
            velecsum         = _fjsp_add_v2r8(velecsum,velec);

            fscal            = felec;

            /* Update vectorial force */
            fix1             = _fjsp_madd_v2r8(dx10,fscal,fix1);
            fiy1             = _fjsp_madd_v2r8(dy10,fscal,fiy1);
            fiz1             = _fjsp_madd_v2r8(dz10,fscal,fiz1);
            
            fjx0             = _fjsp_madd_v2r8(dx10,fscal,fjx0);
            fjy0             = _fjsp_madd_v2r8(dy10,fscal,fjy0);
            fjz0             = _fjsp_madd_v2r8(dz10,fscal,fjz0);

            /**************************
             * CALCULATE INTERACTIONS *
             **************************/

            /* Compute parameters for interactions between i and j atoms */
            qq20             = _fjsp_mul_v2r8(iq2,jq0);

            /* REACTION-FIELD ELECTROSTATICS */
            velec            = _fjsp_mul_v2r8(qq20,_fjsp_sub_v2r8(_fjsp_madd_v2r8(krf,rsq20,rinv20),crf));
            felec            = _fjsp_mul_v2r8(qq20,_fjsp_msub_v2r8(rinv20,rinvsq20,krf2));

            /* Update potential sum for this i atom from the interaction with this j atom. */
            velecsum         = _fjsp_add_v2r8(velecsum,velec);

            fscal            = felec;

            /* Update vectorial force */
            fix2             = _fjsp_madd_v2r8(dx20,fscal,fix2);
            fiy2             = _fjsp_madd_v2r8(dy20,fscal,fiy2);
            fiz2             = _fjsp_madd_v2r8(dz20,fscal,fiz2);
            
            fjx0             = _fjsp_madd_v2r8(dx20,fscal,fjx0);
            fjy0             = _fjsp_madd_v2r8(dy20,fscal,fjy0);
            fjz0             = _fjsp_madd_v2r8(dz20,fscal,fjz0);

            gmx_fjsp_decrement_1rvec_2ptr_swizzle_v2r8(f+j_coord_offsetA,f+j_coord_offsetB,fjx0,fjy0,fjz0);

            /* Inner loop uses 120 flops */
        }

        if(jidx<j_index_end)
        {

            jnrA             = jjnr[jidx];
            j_coord_offsetA  = DIM*jnrA;

            /* load j atom coordinates */
            gmx_fjsp_load_1rvec_1ptr_swizzle_v2r8(x+j_coord_offsetA,
                                              &jx0,&jy0,&jz0);

            /* Calculate displacement vector */
            dx00             = _fjsp_sub_v2r8(ix0,jx0);
            dy00             = _fjsp_sub_v2r8(iy0,jy0);
            dz00             = _fjsp_sub_v2r8(iz0,jz0);
            dx10             = _fjsp_sub_v2r8(ix1,jx0);
            dy10             = _fjsp_sub_v2r8(iy1,jy0);
            dz10             = _fjsp_sub_v2r8(iz1,jz0);
            dx20             = _fjsp_sub_v2r8(ix2,jx0);
            dy20             = _fjsp_sub_v2r8(iy2,jy0);
            dz20             = _fjsp_sub_v2r8(iz2,jz0);

            /* Calculate squared distance and things based on it */
            rsq00            = gmx_fjsp_calc_rsq_v2r8(dx00,dy00,dz00);
            rsq10            = gmx_fjsp_calc_rsq_v2r8(dx10,dy10,dz10);
            rsq20            = gmx_fjsp_calc_rsq_v2r8(dx20,dy20,dz20);

            rinv00           = gmx_fjsp_invsqrt_v2r8(rsq00);
            rinv10           = gmx_fjsp_invsqrt_v2r8(rsq10);
            rinv20           = gmx_fjsp_invsqrt_v2r8(rsq20);

            rinvsq00         = _fjsp_mul_v2r8(rinv00,rinv00);
            rinvsq10         = _fjsp_mul_v2r8(rinv10,rinv10);
            rinvsq20         = _fjsp_mul_v2r8(rinv20,rinv20);

            /* Load parameters for j particles */
            jq0              = _fjsp_loadl_v2r8(_fjsp_setzero_v2r8(),charge+jnrA+0);
            vdwjidx0A        = 2*vdwtype[jnrA+0];

            fjx0             = _fjsp_setzero_v2r8();
            fjy0             = _fjsp_setzero_v2r8();
            fjz0             = _fjsp_setzero_v2r8();

            /**************************
             * CALCULATE INTERACTIONS *
             **************************/

            /* Compute parameters for interactions between i and j atoms */
            qq00             = _fjsp_mul_v2r8(iq0,jq0);
            gmx_fjsp_load_1pair_swizzle_v2r8(vdwparam+vdwioffset0+vdwjidx0A,&c6_00,&c12_00);

            /* REACTION-FIELD ELECTROSTATICS */
            velec            = _fjsp_mul_v2r8(qq00,_fjsp_sub_v2r8(_fjsp_madd_v2r8(krf,rsq00,rinv00),crf));
            felec            = _fjsp_mul_v2r8(qq00,_fjsp_msub_v2r8(rinv00,rinvsq00,krf2));

            /* LENNARD-JONES DISPERSION/REPULSION */

            rinvsix          = _fjsp_mul_v2r8(_fjsp_mul_v2r8(rinvsq00,rinvsq00),rinvsq00);
            vvdw6            = _fjsp_mul_v2r8(c6_00,rinvsix);
            vvdw12           = _fjsp_mul_v2r8(c12_00,_fjsp_mul_v2r8(rinvsix,rinvsix));
            vvdw             = _fjsp_msub_v2r8( vvdw12,one_twelfth, _fjsp_mul_v2r8(vvdw6,one_sixth) );
            fvdw             = _fjsp_mul_v2r8(_fjsp_sub_v2r8(vvdw12,vvdw6),rinvsq00);

            /* Update potential sum for this i atom from the interaction with this j atom. */
            velec            = _fjsp_unpacklo_v2r8(velec,_fjsp_setzero_v2r8());
            velecsum         = _fjsp_add_v2r8(velecsum,velec);
            vvdw             = _fjsp_unpacklo_v2r8(vvdw,_fjsp_setzero_v2r8());
            vvdwsum          = _fjsp_add_v2r8(vvdwsum,vvdw);

            fscal            = _fjsp_add_v2r8(felec,fvdw);

            fscal            = _fjsp_unpacklo_v2r8(fscal,_fjsp_setzero_v2r8());

            /* Update vectorial force */
            fix0             = _fjsp_madd_v2r8(dx00,fscal,fix0);
            fiy0             = _fjsp_madd_v2r8(dy00,fscal,fiy0);
            fiz0             = _fjsp_madd_v2r8(dz00,fscal,fiz0);
            
            fjx0             = _fjsp_madd_v2r8(dx00,fscal,fjx0);
            fjy0             = _fjsp_madd_v2r8(dy00,fscal,fjy0);
            fjz0             = _fjsp_madd_v2r8(dz00,fscal,fjz0);

            /**************************
             * CALCULATE INTERACTIONS *
             **************************/

            /* Compute parameters for interactions between i and j atoms */
            qq10             = _fjsp_mul_v2r8(iq1,jq0);

            /* REACTION-FIELD ELECTROSTATICS */
            velec            = _fjsp_mul_v2r8(qq10,_fjsp_sub_v2r8(_fjsp_madd_v2r8(krf,rsq10,rinv10),crf));
            felec            = _fjsp_mul_v2r8(qq10,_fjsp_msub_v2r8(rinv10,rinvsq10,krf2));

            /* Update potential sum for this i atom from the interaction with this j atom. */
            velec            = _fjsp_unpacklo_v2r8(velec,_fjsp_setzero_v2r8());
            velecsum         = _fjsp_add_v2r8(velecsum,velec);

            fscal            = felec;

            fscal            = _fjsp_unpacklo_v2r8(fscal,_fjsp_setzero_v2r8());

            /* Update vectorial force */
            fix1             = _fjsp_madd_v2r8(dx10,fscal,fix1);
            fiy1             = _fjsp_madd_v2r8(dy10,fscal,fiy1);
            fiz1             = _fjsp_madd_v2r8(dz10,fscal,fiz1);
            
            fjx0             = _fjsp_madd_v2r8(dx10,fscal,fjx0);
            fjy0             = _fjsp_madd_v2r8(dy10,fscal,fjy0);
            fjz0             = _fjsp_madd_v2r8(dz10,fscal,fjz0);

            /**************************
             * CALCULATE INTERACTIONS *
             **************************/

            /* Compute parameters for interactions between i and j atoms */
            qq20             = _fjsp_mul_v2r8(iq2,jq0);

            /* REACTION-FIELD ELECTROSTATICS */
            velec            = _fjsp_mul_v2r8(qq20,_fjsp_sub_v2r8(_fjsp_madd_v2r8(krf,rsq20,rinv20),crf));
            felec            = _fjsp_mul_v2r8(qq20,_fjsp_msub_v2r8(rinv20,rinvsq20,krf2));

            /* Update potential sum for this i atom from the interaction with this j atom. */
            velec            = _fjsp_unpacklo_v2r8(velec,_fjsp_setzero_v2r8());
            velecsum         = _fjsp_add_v2r8(velecsum,velec);

            fscal            = felec;

            fscal            = _fjsp_unpacklo_v2r8(fscal,_fjsp_setzero_v2r8());

            /* Update vectorial force */
            fix2             = _fjsp_madd_v2r8(dx20,fscal,fix2);
            fiy2             = _fjsp_madd_v2r8(dy20,fscal,fiy2);
            fiz2             = _fjsp_madd_v2r8(dz20,fscal,fiz2);
            
            fjx0             = _fjsp_madd_v2r8(dx20,fscal,fjx0);
            fjy0             = _fjsp_madd_v2r8(dy20,fscal,fjy0);
            fjz0             = _fjsp_madd_v2r8(dz20,fscal,fjz0);

            gmx_fjsp_decrement_1rvec_1ptr_swizzle_v2r8(f+j_coord_offsetA,fjx0,fjy0,fjz0);

            /* Inner loop uses 120 flops */
        }

        /* End of innermost loop */

        gmx_fjsp_update_iforce_3atom_swizzle_v2r8(fix0,fiy0,fiz0,fix1,fiy1,fiz1,fix2,fiy2,fiz2,
                                              f+i_coord_offset,fshift+i_shift_offset);

        ggid                        = gid[iidx];
        /* Update potential energies */
        gmx_fjsp_update_1pot_v2r8(velecsum,kernel_data->energygrp_elec+ggid);
        gmx_fjsp_update_1pot_v2r8(vvdwsum,kernel_data->energygrp_vdw+ggid);

        /* Increment number of inner iterations */
        inneriter                  += j_index_end - j_index_start;

        /* Outer loop uses 20 flops */
    }

    /* Increment number of outer iterations */
    outeriter        += nri;

    /* Update outer/inner flops */

    inc_nrnb(nrnb,eNR_NBKERNEL_ELEC_VDW_W3_VF,outeriter*20 + inneriter*120);
}
/*
 * Gromacs nonbonded kernel:   nb_kernel_ElecRF_VdwLJ_GeomW3P1_F_sparc64_hpc_ace_double
 * Electrostatics interaction: ReactionField
 * VdW interaction:            LennardJones
 * Geometry:                   Water3-Particle
 * Calculate force/pot:        Force
 */
void
nb_kernel_ElecRF_VdwLJ_GeomW3P1_F_sparc64_hpc_ace_double
                    (t_nblist * gmx_restrict                nlist,
                     rvec * gmx_restrict                    xx,
                     rvec * gmx_restrict                    ff,
                     t_forcerec * gmx_restrict              fr,
                     t_mdatoms * gmx_restrict               mdatoms,
                     nb_kernel_data_t * gmx_restrict        kernel_data,
                     t_nrnb * gmx_restrict                  nrnb)
{
    /* Suffixes 0,1,2,3 refer to particle indices for waters in the inner or outer loop, or
     * just 0 for non-waters.
     * Suffixes A,B refer to j loop unrolling done with double precision SIMD, e.g. for the two different
     * jnr indices corresponding to data put in the four positions in the SIMD register.
     */
    int              i_shift_offset,i_coord_offset,outeriter,inneriter;
    int              j_index_start,j_index_end,jidx,nri,inr,ggid,iidx;
    int              jnrA,jnrB;
    int              j_coord_offsetA,j_coord_offsetB;
    int              *iinr,*jindex,*jjnr,*shiftidx,*gid;
    real             rcutoff_scalar;
    real             *shiftvec,*fshift,*x,*f;
    _fjsp_v2r8       tx,ty,tz,fscal,rcutoff,rcutoff2,jidxall;
    int              vdwioffset0;
    _fjsp_v2r8       ix0,iy0,iz0,fix0,fiy0,fiz0,iq0,isai0;
    int              vdwioffset1;
    _fjsp_v2r8       ix1,iy1,iz1,fix1,fiy1,fiz1,iq1,isai1;
    int              vdwioffset2;
    _fjsp_v2r8       ix2,iy2,iz2,fix2,fiy2,fiz2,iq2,isai2;
    int              vdwjidx0A,vdwjidx0B;
    _fjsp_v2r8       jx0,jy0,jz0,fjx0,fjy0,fjz0,jq0,isaj0;
    _fjsp_v2r8       dx00,dy00,dz00,rsq00,rinv00,rinvsq00,r00,qq00,c6_00,c12_00;
    _fjsp_v2r8       dx10,dy10,dz10,rsq10,rinv10,rinvsq10,r10,qq10,c6_10,c12_10;
    _fjsp_v2r8       dx20,dy20,dz20,rsq20,rinv20,rinvsq20,r20,qq20,c6_20,c12_20;
    _fjsp_v2r8       velec,felec,velecsum,facel,crf,krf,krf2;
    real             *charge;
    int              nvdwtype;
    _fjsp_v2r8       rinvsix,rvdw,vvdw,vvdw6,vvdw12,fvdw,fvdw6,fvdw12,vvdwsum,sh_vdw_invrcut6;
    int              *vdwtype;
    real             *vdwparam;
    _fjsp_v2r8       one_sixth   = gmx_fjsp_set1_v2r8(1.0/6.0);
    _fjsp_v2r8       one_twelfth = gmx_fjsp_set1_v2r8(1.0/12.0);
    _fjsp_v2r8       itab_tmp;
    _fjsp_v2r8       dummy_mask,cutoff_mask;
    _fjsp_v2r8       one     = gmx_fjsp_set1_v2r8(1.0);
    _fjsp_v2r8       two     = gmx_fjsp_set1_v2r8(2.0);
    union { _fjsp_v2r8 simd; long long int i[2]; } vfconv,gbconv,ewconv;

    x                = xx[0];
    f                = ff[0];

    nri              = nlist->nri;
    iinr             = nlist->iinr;
    jindex           = nlist->jindex;
    jjnr             = nlist->jjnr;
    shiftidx         = nlist->shift;
    gid              = nlist->gid;
    shiftvec         = fr->shift_vec[0];
    fshift           = fr->fshift[0];
    facel            = gmx_fjsp_set1_v2r8(fr->epsfac);
    charge           = mdatoms->chargeA;
    krf              = gmx_fjsp_set1_v2r8(fr->ic->k_rf);
    krf2             = gmx_fjsp_set1_v2r8(fr->ic->k_rf*2.0);
    crf              = gmx_fjsp_set1_v2r8(fr->ic->c_rf);
    nvdwtype         = fr->ntype;
    vdwparam         = fr->nbfp;
    vdwtype          = mdatoms->typeA;

    /* Setup water-specific parameters */
    inr              = nlist->iinr[0];
    iq0              = _fjsp_mul_v2r8(facel,gmx_fjsp_set1_v2r8(charge[inr+0]));
    iq1              = _fjsp_mul_v2r8(facel,gmx_fjsp_set1_v2r8(charge[inr+1]));
    iq2              = _fjsp_mul_v2r8(facel,gmx_fjsp_set1_v2r8(charge[inr+2]));
    vdwioffset0      = 2*nvdwtype*vdwtype[inr+0];

    /* Avoid stupid compiler warnings */
    jnrA = jnrB = 0;
    j_coord_offsetA = 0;
    j_coord_offsetB = 0;

    outeriter        = 0;
    inneriter        = 0;

    /* Start outer loop over neighborlists */
    for(iidx=0; iidx<nri; iidx++)
    {
        /* Load shift vector for this list */
        i_shift_offset   = DIM*shiftidx[iidx];

        /* Load limits for loop over neighbors */
        j_index_start    = jindex[iidx];
        j_index_end      = jindex[iidx+1];

        /* Get outer coordinate index */
        inr              = iinr[iidx];
        i_coord_offset   = DIM*inr;

        /* Load i particle coords and add shift vector */
        gmx_fjsp_load_shift_and_3rvec_broadcast_v2r8(shiftvec+i_shift_offset,x+i_coord_offset,
                                                 &ix0,&iy0,&iz0,&ix1,&iy1,&iz1,&ix2,&iy2,&iz2);

        fix0             = _fjsp_setzero_v2r8();
        fiy0             = _fjsp_setzero_v2r8();
        fiz0             = _fjsp_setzero_v2r8();
        fix1             = _fjsp_setzero_v2r8();
        fiy1             = _fjsp_setzero_v2r8();
        fiz1             = _fjsp_setzero_v2r8();
        fix2             = _fjsp_setzero_v2r8();
        fiy2             = _fjsp_setzero_v2r8();
        fiz2             = _fjsp_setzero_v2r8();

        /* Start inner kernel loop */
        for(jidx=j_index_start; jidx<j_index_end-1; jidx+=2)
        {

            /* Get j neighbor index, and coordinate index */
            jnrA             = jjnr[jidx];
            jnrB             = jjnr[jidx+1];
            j_coord_offsetA  = DIM*jnrA;
            j_coord_offsetB  = DIM*jnrB;

            /* load j atom coordinates */
            gmx_fjsp_load_1rvec_2ptr_swizzle_v2r8(x+j_coord_offsetA,x+j_coord_offsetB,
                                              &jx0,&jy0,&jz0);

            /* Calculate displacement vector */
            dx00             = _fjsp_sub_v2r8(ix0,jx0);
            dy00             = _fjsp_sub_v2r8(iy0,jy0);
            dz00             = _fjsp_sub_v2r8(iz0,jz0);
            dx10             = _fjsp_sub_v2r8(ix1,jx0);
            dy10             = _fjsp_sub_v2r8(iy1,jy0);
            dz10             = _fjsp_sub_v2r8(iz1,jz0);
            dx20             = _fjsp_sub_v2r8(ix2,jx0);
            dy20             = _fjsp_sub_v2r8(iy2,jy0);
            dz20             = _fjsp_sub_v2r8(iz2,jz0);

            /* Calculate squared distance and things based on it */
            rsq00            = gmx_fjsp_calc_rsq_v2r8(dx00,dy00,dz00);
            rsq10            = gmx_fjsp_calc_rsq_v2r8(dx10,dy10,dz10);
            rsq20            = gmx_fjsp_calc_rsq_v2r8(dx20,dy20,dz20);

            rinv00           = gmx_fjsp_invsqrt_v2r8(rsq00);
            rinv10           = gmx_fjsp_invsqrt_v2r8(rsq10);
            rinv20           = gmx_fjsp_invsqrt_v2r8(rsq20);

            rinvsq00         = _fjsp_mul_v2r8(rinv00,rinv00);
            rinvsq10         = _fjsp_mul_v2r8(rinv10,rinv10);
            rinvsq20         = _fjsp_mul_v2r8(rinv20,rinv20);

            /* Load parameters for j particles */
            jq0              = gmx_fjsp_load_2real_swizzle_v2r8(charge+jnrA+0,charge+jnrB+0);
            vdwjidx0A        = 2*vdwtype[jnrA+0];
            vdwjidx0B        = 2*vdwtype[jnrB+0];

            fjx0             = _fjsp_setzero_v2r8();
            fjy0             = _fjsp_setzero_v2r8();
            fjz0             = _fjsp_setzero_v2r8();

            /**************************
             * CALCULATE INTERACTIONS *
             **************************/

            /* Compute parameters for interactions between i and j atoms */
            qq00             = _fjsp_mul_v2r8(iq0,jq0);
            gmx_fjsp_load_2pair_swizzle_v2r8(vdwparam+vdwioffset0+vdwjidx0A,
                                         vdwparam+vdwioffset0+vdwjidx0B,&c6_00,&c12_00);

            /* REACTION-FIELD ELECTROSTATICS */
            felec            = _fjsp_mul_v2r8(qq00,_fjsp_msub_v2r8(rinv00,rinvsq00,krf2));

            /* LENNARD-JONES DISPERSION/REPULSION */

            rinvsix          = _fjsp_mul_v2r8(_fjsp_mul_v2r8(rinvsq00,rinvsq00),rinvsq00);
            fvdw             = _fjsp_mul_v2r8(_fjsp_msub_v2r8(c12_00,rinvsix,c6_00),_fjsp_mul_v2r8(rinvsix,rinvsq00));

            fscal            = _fjsp_add_v2r8(felec,fvdw);

            /* Update vectorial force */
            fix0             = _fjsp_madd_v2r8(dx00,fscal,fix0);
            fiy0             = _fjsp_madd_v2r8(dy00,fscal,fiy0);
            fiz0             = _fjsp_madd_v2r8(dz00,fscal,fiz0);
            
            fjx0             = _fjsp_madd_v2r8(dx00,fscal,fjx0);
            fjy0             = _fjsp_madd_v2r8(dy00,fscal,fjy0);
            fjz0             = _fjsp_madd_v2r8(dz00,fscal,fjz0);

            /**************************
             * CALCULATE INTERACTIONS *
             **************************/

            /* Compute parameters for interactions between i and j atoms */
            qq10             = _fjsp_mul_v2r8(iq1,jq0);

            /* REACTION-FIELD ELECTROSTATICS */
            felec            = _fjsp_mul_v2r8(qq10,_fjsp_msub_v2r8(rinv10,rinvsq10,krf2));

            fscal            = felec;

            /* Update vectorial force */
            fix1             = _fjsp_madd_v2r8(dx10,fscal,fix1);
            fiy1             = _fjsp_madd_v2r8(dy10,fscal,fiy1);
            fiz1             = _fjsp_madd_v2r8(dz10,fscal,fiz1);
            
            fjx0             = _fjsp_madd_v2r8(dx10,fscal,fjx0);
            fjy0             = _fjsp_madd_v2r8(dy10,fscal,fjy0);
            fjz0             = _fjsp_madd_v2r8(dz10,fscal,fjz0);

            /**************************
             * CALCULATE INTERACTIONS *
             **************************/

            /* Compute parameters for interactions between i and j atoms */
            qq20             = _fjsp_mul_v2r8(iq2,jq0);

            /* REACTION-FIELD ELECTROSTATICS */
            felec            = _fjsp_mul_v2r8(qq20,_fjsp_msub_v2r8(rinv20,rinvsq20,krf2));

            fscal            = felec;

            /* Update vectorial force */
            fix2             = _fjsp_madd_v2r8(dx20,fscal,fix2);
            fiy2             = _fjsp_madd_v2r8(dy20,fscal,fiy2);
            fiz2             = _fjsp_madd_v2r8(dz20,fscal,fiz2);
            
            fjx0             = _fjsp_madd_v2r8(dx20,fscal,fjx0);
            fjy0             = _fjsp_madd_v2r8(dy20,fscal,fjy0);
            fjz0             = _fjsp_madd_v2r8(dz20,fscal,fjz0);

            gmx_fjsp_decrement_1rvec_2ptr_swizzle_v2r8(f+j_coord_offsetA,f+j_coord_offsetB,fjx0,fjy0,fjz0);

            /* Inner loop uses 100 flops */
        }

        if(jidx<j_index_end)
        {

            jnrA             = jjnr[jidx];
            j_coord_offsetA  = DIM*jnrA;

            /* load j atom coordinates */
            gmx_fjsp_load_1rvec_1ptr_swizzle_v2r8(x+j_coord_offsetA,
                                              &jx0,&jy0,&jz0);

            /* Calculate displacement vector */
            dx00             = _fjsp_sub_v2r8(ix0,jx0);
            dy00             = _fjsp_sub_v2r8(iy0,jy0);
            dz00             = _fjsp_sub_v2r8(iz0,jz0);
            dx10             = _fjsp_sub_v2r8(ix1,jx0);
            dy10             = _fjsp_sub_v2r8(iy1,jy0);
            dz10             = _fjsp_sub_v2r8(iz1,jz0);
            dx20             = _fjsp_sub_v2r8(ix2,jx0);
            dy20             = _fjsp_sub_v2r8(iy2,jy0);
            dz20             = _fjsp_sub_v2r8(iz2,jz0);

            /* Calculate squared distance and things based on it */
            rsq00            = gmx_fjsp_calc_rsq_v2r8(dx00,dy00,dz00);
            rsq10            = gmx_fjsp_calc_rsq_v2r8(dx10,dy10,dz10);
            rsq20            = gmx_fjsp_calc_rsq_v2r8(dx20,dy20,dz20);

            rinv00           = gmx_fjsp_invsqrt_v2r8(rsq00);
            rinv10           = gmx_fjsp_invsqrt_v2r8(rsq10);
            rinv20           = gmx_fjsp_invsqrt_v2r8(rsq20);

            rinvsq00         = _fjsp_mul_v2r8(rinv00,rinv00);
            rinvsq10         = _fjsp_mul_v2r8(rinv10,rinv10);
            rinvsq20         = _fjsp_mul_v2r8(rinv20,rinv20);

            /* Load parameters for j particles */
            jq0              = _fjsp_loadl_v2r8(_fjsp_setzero_v2r8(),charge+jnrA+0);
            vdwjidx0A        = 2*vdwtype[jnrA+0];

            fjx0             = _fjsp_setzero_v2r8();
            fjy0             = _fjsp_setzero_v2r8();
            fjz0             = _fjsp_setzero_v2r8();

            /**************************
             * CALCULATE INTERACTIONS *
             **************************/

            /* Compute parameters for interactions between i and j atoms */
            qq00             = _fjsp_mul_v2r8(iq0,jq0);
            gmx_fjsp_load_1pair_swizzle_v2r8(vdwparam+vdwioffset0+vdwjidx0A,&c6_00,&c12_00);

            /* REACTION-FIELD ELECTROSTATICS */
            felec            = _fjsp_mul_v2r8(qq00,_fjsp_msub_v2r8(rinv00,rinvsq00,krf2));

            /* LENNARD-JONES DISPERSION/REPULSION */

            rinvsix          = _fjsp_mul_v2r8(_fjsp_mul_v2r8(rinvsq00,rinvsq00),rinvsq00);
            fvdw             = _fjsp_mul_v2r8(_fjsp_msub_v2r8(c12_00,rinvsix,c6_00),_fjsp_mul_v2r8(rinvsix,rinvsq00));

            fscal            = _fjsp_add_v2r8(felec,fvdw);

            fscal            = _fjsp_unpacklo_v2r8(fscal,_fjsp_setzero_v2r8());

            /* Update vectorial force */
            fix0             = _fjsp_madd_v2r8(dx00,fscal,fix0);
            fiy0             = _fjsp_madd_v2r8(dy00,fscal,fiy0);
            fiz0             = _fjsp_madd_v2r8(dz00,fscal,fiz0);
            
            fjx0             = _fjsp_madd_v2r8(dx00,fscal,fjx0);
            fjy0             = _fjsp_madd_v2r8(dy00,fscal,fjy0);
            fjz0             = _fjsp_madd_v2r8(dz00,fscal,fjz0);

            /**************************
             * CALCULATE INTERACTIONS *
             **************************/

            /* Compute parameters for interactions between i and j atoms */
            qq10             = _fjsp_mul_v2r8(iq1,jq0);

            /* REACTION-FIELD ELECTROSTATICS */
            felec            = _fjsp_mul_v2r8(qq10,_fjsp_msub_v2r8(rinv10,rinvsq10,krf2));

            fscal            = felec;

            fscal            = _fjsp_unpacklo_v2r8(fscal,_fjsp_setzero_v2r8());

            /* Update vectorial force */
            fix1             = _fjsp_madd_v2r8(dx10,fscal,fix1);
            fiy1             = _fjsp_madd_v2r8(dy10,fscal,fiy1);
            fiz1             = _fjsp_madd_v2r8(dz10,fscal,fiz1);
            
            fjx0             = _fjsp_madd_v2r8(dx10,fscal,fjx0);
            fjy0             = _fjsp_madd_v2r8(dy10,fscal,fjy0);
            fjz0             = _fjsp_madd_v2r8(dz10,fscal,fjz0);

            /**************************
             * CALCULATE INTERACTIONS *
             **************************/

            /* Compute parameters for interactions between i and j atoms */
            qq20             = _fjsp_mul_v2r8(iq2,jq0);

            /* REACTION-FIELD ELECTROSTATICS */
            felec            = _fjsp_mul_v2r8(qq20,_fjsp_msub_v2r8(rinv20,rinvsq20,krf2));

            fscal            = felec;

            fscal            = _fjsp_unpacklo_v2r8(fscal,_fjsp_setzero_v2r8());

            /* Update vectorial force */
            fix2             = _fjsp_madd_v2r8(dx20,fscal,fix2);
            fiy2             = _fjsp_madd_v2r8(dy20,fscal,fiy2);
            fiz2             = _fjsp_madd_v2r8(dz20,fscal,fiz2);
            
            fjx0             = _fjsp_madd_v2r8(dx20,fscal,fjx0);
            fjy0             = _fjsp_madd_v2r8(dy20,fscal,fjy0);
            fjz0             = _fjsp_madd_v2r8(dz20,fscal,fjz0);

            gmx_fjsp_decrement_1rvec_1ptr_swizzle_v2r8(f+j_coord_offsetA,fjx0,fjy0,fjz0);

            /* Inner loop uses 100 flops */
        }

        /* End of innermost loop */

        gmx_fjsp_update_iforce_3atom_swizzle_v2r8(fix0,fiy0,fiz0,fix1,fiy1,fiz1,fix2,fiy2,fiz2,
                                              f+i_coord_offset,fshift+i_shift_offset);

        /* Increment number of inner iterations */
        inneriter                  += j_index_end - j_index_start;

        /* Outer loop uses 18 flops */
    }

    /* Increment number of outer iterations */
    outeriter        += nri;

    /* Update outer/inner flops */

    inc_nrnb(nrnb,eNR_NBKERNEL_ELEC_VDW_W3_F,outeriter*18 + inneriter*100);
}
